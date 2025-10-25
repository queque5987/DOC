import os
import time

import numpy as np
import onnxruntime as rt
import pandas as pd
import xgboost as xgb
from onnxmltools.convert import convert_xgboost
from onnxmltools.convert.common.data_types import FloatTensorType
from sklearn.metrics import mean_squared_error
from sklearn.model_selection import train_test_split

DATA_DIR = "Data_V3_noradian/"
TARGET_COLUMN = "PlayerRelativeDirectionRadian"
ONNX_MODEL_PATH = "xgboost_doc_model_V3_noradian.onnx"
N_ROWS_PER_FILE = 10
AUGMENTATION_FACTOR = 10

IMPORTANCE_COLUMN_NAMES = ['PlayerForwardRadian','PlayerVelocity_X','PlayerVelocity_Y',
                           'RelativeDistance','Dist_from_Top','Dist_from_Bottom',
                           'Dist_from_Left','Dist_from_Right','PlayerHP','PlayerStamina']

def load_and_prepare_data_v3(data_dir):
    file_count = 0
    try:
        files = [os.path.join(data_dir, f) for f in os.listdir(data_dir) if f.endswith(".csv")]
        if not files:
            print("No CSV files found.")
            return None, None, 0

        all_rows = []
        all_y = []

        for f_path in files:
            df = pd.read_csv(f_path)

            if not all(col in df.columns for col in IMPORTANCE_COLUMN_NAMES) or len(df) < N_ROWS_PER_FILE:
                print(f"Skipping file {f_path} due to issues.")
                continue

            file_count += 1
            df_slice = df.head(N_ROWS_PER_FILE)
            
            stds = {col: df_slice[col].std() for col in IMPORTANCE_COLUMN_NAMES}

            for i_aug in range(AUGMENTATION_FACTOR):
                augmented_slice = df_slice.copy()
                if i_aug > 0:
                    for col in IMPORTANCE_COLUMN_NAMES:
                        noise_std = stds[col] * 0.05 
                        noise = np.random.normal(0, noise_std, size=augmented_slice[col].shape)
                        augmented_slice[col] += noise

                new_row = {}
                n = 0
                for i in range(N_ROWS_PER_FILE):
                    for col in IMPORTANCE_COLUMN_NAMES:
                        if col == TARGET_COLUMN:
                            continue
                        new_row[f"f{n}"] = augmented_slice[col].iloc[i]
                        n += 1
                
                all_rows.append(new_row)
                all_y.append(augmented_slice[TARGET_COLUMN].iloc[N_ROWS_PER_FILE - 1])

        if not all_rows:
            print("No valid data processed.")
            return None, None, 0

        x_df = pd.DataFrame(all_rows)
        y_series = pd.Series(all_y, name=TARGET_COLUMN)

        return x_df, y_series, file_count

    except Exception as e:
        print(f"{e}")
        return None, None, 0

def main():
    x_df, y, count = load_and_prepare_data_v3(DATA_DIR)

    if x_df is None or y is None or x_df.empty:
        print("Data loading failed.")
        return
    
    save_df = x_df.copy()
    save_df["output"] = y
    save_df.to_csv("processed_dataset_V3_augmented.csv", index=False)

    x_train, x_test, y_train, y_test = train_test_split(
        x_df, y, test_size=0.2, random_state=42
    )
    print(f"\nTraining Data: {x_train.shape}, Validation Data: {x_test.shape}")

    model = xgb.XGBRegressor(
        n_estimators=1000,
        learning_rate=0.05,
        max_depth=7,
        subsample=0.8,
        colsample_bytree=0.8,
        objective="reg:squarederror",
        early_stopping_rounds=50,
        random_state=42,
        n_jobs=-1,
    )

    model.fit(x_train, y_train, eval_set=[(x_test, y_test)], verbose=50)
    print("Training Complete.")

    importances = model.feature_importances_
    
    feature_names = []
    for i in range(N_ROWS_PER_FILE):
        for name in IMPORTANCE_COLUMN_NAMES:
            feature_names.append(f"{name}_{i}")
    
    if len(feature_names) != len(importances):
         feature_names = [f'f{i}' for i in range(len(importances))]

    feature_importance_df = pd.DataFrame({"Feature": feature_names, "Importance": importances})
    feature_importance_df = feature_importance_df.sort_values(by='Feature', ascending=False)
    feature_importance_df.to_csv(f"processed_dataset_importance_V4_aug{AUGMENTATION_FACTOR}_{count}data.csv", index=False)

    predictions = model.predict(x_test)
    mse = mean_squared_error(y_test, predictions)
    print(f"\nMSE: {mse:.6f}")

    num_features = x_train.shape[1]
    initial_type = [("float_input", FloatTensorType([None, num_features]))]

    ONNX_MODEL_PATH = f"xgboost_doc_model_V4_aug{AUGMENTATION_FACTOR}_{count}.onnx"

    try:
        onx_model = convert_xgboost(model, initial_types=initial_type, target_opset=9)
        with open(ONNX_MODEL_PATH, "wb") as f:
            f.write(onx_model.SerializeToString())
        print(f"ONNX Model Saved to {ONNX_MODEL_PATH}")
    except Exception as e:
        print(f"ONNX Model Save Failed : {e}")
        return

    try:
        sess = rt.InferenceSession(ONNX_MODEL_PATH)
        input_name = sess.get_inputs()[0].name
        label_name = sess.get_outputs()[0].name

        x_test_np = x_test.to_numpy(dtype=np.float32)
        onnx_predictions = sess.run([label_name], {input_name: x_test_np})[0]

        np.testing.assert_allclose(predictions, onnx_predictions.flatten(), rtol=5e-3)
        print("ONNX Validation Successful.")
    except Exception as e:
        print(f"ONNX Validation Error : {e}")


if __name__ == "__main__":
    main()
