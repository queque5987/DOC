
import os
import pandas as pd
import numpy as np
import xgboost as xgb
from sklearn.metrics import mean_squared_error
from sklearn.model_selection import train_test_split
import onnxruntime as rt
from onnxmltools.convert import convert_xgboost
from onnxmltools.convert.common.data_types import FloatTensorType
import time

DATA_DIR = 'Data_V2_Radian/'
TARGET_COLUMN = 'PlayerRelativeDirectionRadian'
ONNX_MODEL_PATH = "xgboost_doc_model_V2.onnx"
N_PAST_TICKS = 10

def create_sequences(data, n_past, target_column):
    X, y = [], []
    
    # 타겟 컬럼을 제외한 피처 컬럼들의 인덱스
    feature_cols = [col for col in data.columns if col != target_column]
    
    for i in range(n_past, len(data)):
        y.append(data[target_column].iloc[i])
        
        # i - n_past 틱의 컬럼 추가
        past_features = data[feature_cols].iloc[i-n_past:i].values.flatten()
        X.append(past_features)
        
    return np.array(X), np.array(y)

def load_and_prepare_data(data_dir, n_past, target_column):
    try:
        files = [os.path.join(data_dir, f) for f in os.listdir(data_dir) if f.endswith('.csv')]
        if not files:
            raise FileNotFoundError()
        all_X, all_y = [], []

        for i, f_path in enumerate(files):
            df = pd.read_csv(f_path)
            
            if target_column not in df.columns:
                continue
            
            X_seq, y_seq = create_sequences(df, n_past, target_column)
            
            if X_seq.shape[0] > 0:
                all_X.append(X_seq)
                all_y.append(y_seq)

        final_X = np.vstack(all_X)
        final_y = np.hstack(all_y)
        return final_X, final_y

    except Exception as e:
        return None, None

def main():    
    # 데이터 로드 및 시퀀스 변환
    X, y = load_and_prepare_data(DATA_DIR, N_PAST_TICKS, TARGET_COLUMN)
    
    if X is None or y is None or X.shape[0] == 0:
        raise FileNotFoundError()

    # 컬럼명 변경
    X_df = pd.DataFrame(X, columns=[f'f{i}' for i in range(X.shape[1])])
    
    # 학습용 테스트용 분리
    X_train, X_test, y_train, y_test = train_test_split(X_df, y, test_size=0.2, random_state=42)
    print(f"\nTraining Data: {X_train.shape}, Validation Data: {X_test.shape}")

    # XGBoost 학습
    model = xgb.XGBRegressor(
        n_estimators=1000,
        learning_rate=0.05,
        max_depth=7,
        subsample=0.8,
        colsample_bytree=0.8,
        objective='reg:squarederror',
        early_stopping_rounds=50,
        random_state=42,
        n_jobs=-1
    )

    model.fit(X_train, y_train, 
              eval_set=[(X_test, y_test)], 
              verbose=50)
    print("Training Complete.")

    # 모델 평가
    predictions = model.predict(X_test)
    mse = mean_squared_error(y_test, predictions)
    print(f"\nMSE: {mse:.6f}")

    # ONNX 변환
    num_features = X_train.shape[1]
    initial_type = [('float_input', FloatTensorType([None, num_features]))]
    
    try:
        onx_model = convert_xgboost(model, initial_types=initial_type, target_opset=9)
        with open(ONNX_MODEL_PATH, "wb") as f:
            f.write(onx_model.SerializeToString())
        print("ONNX Model Saved.")
    except Exception as e:
        print(f"ONNX Model Save Failed : {e}")
        return

    # 모델 Validation
    try:
        sess = rt.InferenceSession(ONNX_MODEL_PATH)
        input_name = sess.get_inputs()[0].name
        label_name = sess.get_outputs()[0].name

        X_test_np = X_test.to_numpy(dtype=np.float32)
        onnx_predictions = sess.run([label_name], {input_name: X_test_np})[0]

        np.testing.assert_allclose(predictions, onnx_predictions.flatten(), rtol=1e-5)
    except Exception as e:
        print(f"ONNX Validation Error : {e}")


if __name__ == '__main__':
    main()
