import { Router, Request, Response } from "express";
import type * as stm32_dht11_reader_type from 'stm32_dht11_reader';
const STM32DHT11Reader = require("bindings")("stm32_dht11_reader") as typeof stm32_dht11_reader_type;
import { EC, PrintErrorCodeString } from "../../shared/error_code/ec_stm32_dht11_reader"
import { ServerRes } from "../../shared/types/ServerRes";

const router = Router();

router.get("/", (req: Request, res: Response<ServerRes>) => {
  return res.status(200).json({
    success: true,
    err: 0,
    message: "[api/] you are in the api base router",
  });
});

router.get("/Init", (req: Request, res: Response<ServerRes>) => {
  const ec = STM32DHT11Reader.Init();
  const isOk = ec === EC.Ok;
  return res.status(200).json({
    success: isOk,
    err: ec,
    message: "[api/Init] " + (isOk ? "Init successfully" : PrintErrorCodeString(ec))
  });
});

router.get("/ReInit", (req: Request, res: Response<ServerRes>) => {
  const ec = STM32DHT11Reader.ReInit();
  const isOk = ec === EC.Ok;
  return res.status(200).json({
    success: isOk,
    err: ec,
    message: "[api/ReInit] " + (isOk ? "ReInit successfully" : PrintErrorCodeString(ec)),
  });
});

router.get("/IsInit", (req: Request, res: Response<ServerRes>) => {
  const isInit = STM32DHT11Reader.IsInit();
  return res.status(200).json({
    success: isInit,
    err: 0,
    message: "[api/IsInit] IsInit return " + isInit,
  });
});

router.get("/UpdateData", (req: Request, res: Response<ServerRes>) => {
  const ec = STM32DHT11Reader.UpdateData();
  const isOk = ec === EC.Ok;
  return res.status(200).json({
    success: isOk,
    err: ec,
    message: "[api/UpdateData] " + (isOk ? "UpdateData successfully" : PrintErrorCodeString(ec))
  });
});

router.get("/GetTemperature", (req: Request, res: Response<ServerRes>) => {
  const tempData = STM32DHT11Reader.GetTemperature();
  return res.status(200).json({
    success: true,
    err: 0,
    message: "[api/GetTemperature] GetTemperature successfully",
    data: { temp: tempData, humidity: -1 }
  });
});

router.get("/GetHumidity", (req: Request, res: Response<ServerRes>) => {
  const humidityData = STM32DHT11Reader.GetHumidity();
  return res.status(200).json({
    success: true,
    err: 0,
    message: "[api/GetHumidity] GetHumidity successfully",
    data: { temp: -1, humidity: humidityData }
  });
});

router.get("/GetData", (req: Request, res: Response<ServerRes>) => {
  const tempData = STM32DHT11Reader.GetTemperature();
  const humidityData = STM32DHT11Reader.GetHumidity();
  return res.status(200).json({
    success: true,
    err: 0,
    message: "[api/GetData] GetData successfully",
    data: { temp: tempData, humidity: humidityData }
  });
});

export default router;
