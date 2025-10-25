from fastapi import FastAPI
from pydantic import BaseModel
from typing import List, Union
import csv
import os
from datetime import datetime

app = FastAPI()

class TimeSeriesDataV3(BaseModel):
    TimeStamp: List[float]
    PlayerForwardRadian: List[float]
    PlayerVelocity_X: List[float]
    PlayerVelocity_Y: List[float]
    PlayerRelativeDirectionRadian: List[float]
    RelativeDistance: List[float]
    Dist_from_Top : List[float]
    Dist_from_Bottom : List[float]
    Dist_from_Left : List[float]
    Dist_from_Right : List[float]
    PlayerHP: List[float]
    PlayerStamina: List[float]

class TimeSeriesDataV2(BaseModel):
    TimeStamp: List[float]
    PlayerForwardRadian: List[float]
    PlayerVelocity: List[float]
    PlayerRelativeDirectionRadian: List[float]
    RelativeDistance: List[float]
    Dist_from_Top : List[float]
    Dist_from_Bottom : List[float]
    Dist_from_Left : List[float]
    Dist_from_Right : List[float]
    PlayerHP: List[float]
    PlayerStamina: List[float]

class TimeSeriesData(BaseModel):
    TimeStamp: List[float]
    PlayerButtonSeries: List[int]
    RelativeDistance: List[float]
    Dist_from_Top : List[float]
    Dist_from_Bottom : List[float]
    Dist_from_Left : List[float]
    Dist_from_Right : List[float]
    PlayerHP: List[float]
    PlayerStamina: List[float]

@app.get("/")
def read_root():
    return {"Active"}

@app.post("/data")
async def receive_data(data: TimeSeriesDataV3):
    data_dir_v2 = "Data_V3_noradian"
    os.makedirs(data_dir_v2, exist_ok=True)

    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    csv_filename_v2 = os.path.join(data_dir_v2, f"data_{timestamp}.csv")

    data_dict_v2 = data.dict()
    headers_v2 = data_dict_v2.keys()
    rows_v2 = zip(*data_dict_v2.values())

    try:
        with open(csv_filename_v2, "w", newline="", encoding="utf-8") as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(headers_v2)
            writer.writerows(rows_v2)
        print(f"Data successfully saved to new file: {csv_filename_v2}")
    except Exception as e:
        print(f"Error writing to CSV: {e}")

    return {"received_data_v2": data.TimeStamp}