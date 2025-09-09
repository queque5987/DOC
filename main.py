from fastapi import FastAPI
from pydantic import BaseModel
from typing import List, Union
import csv
import os
from datetime import datetime

app = FastAPI()

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
async def receive_data(data: TimeSeriesData):
    data_dir = "Data"
    os.makedirs(data_dir, exist_ok=True)

    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    csv_filename = os.path.join(data_dir, f"data_{timestamp}.csv")

    data_dict = data.dict()
    headers = data_dict.keys()
    rows = zip(*data_dict.values())

    try:
        with open(csv_filename, "w", newline="", encoding="utf-8") as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(headers)
            writer.writerows(rows)
        print(f"Data successfully saved to new file: {csv_filename}")
    except Exception as e:
        print(f"Error writing to CSV: {e}")

    for f in data.PlayerButtonSeries:
        print(f"TimeSeriese : {f}")
    return {"received_data": data.TimeStamp}