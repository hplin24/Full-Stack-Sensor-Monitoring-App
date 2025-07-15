import { Router, Request, Response } from "express";
const stm32_dht11 = require("bindings")("stm32_dht11");
import { ApiResponse } from "../../types/api-response";

const router = Router();

router.get("/", (req: Request, res: Response<ApiResponse>) => {
  res.status(200).json({
    success: true,
    err: 0,
    message: "[api/] you are in the api base router",
  });
});

router.get("/initialize", (req: Request, res: Response<ApiResponse>) => {
  let ec = stm32_dht11.Initialize();
  if (ec === 9) {
    return res.status(200).json({
      success: true,
      err: 0,
      message: "[api/initialize] stm32_dht11 init successfully",
    });
  }

  res.status(200).json({
    success: false,
    err: ec,
    message: "[api/initialize] stm32_dht11 init failed, error: " + ec,
  });
});

router.get("/getdata", (req: Request, res: Response) => {
  const newData: { temp: number; rh: number } = { temp: 0, rh: 0 };
  let ec = stm32_dht11.GetData(newData);
  if (ec === 9) {
    return res.status(200).json({
      success: true,
      err: 0,
      message: "[api/getdata] stm32_dht11 GetData successfully",
      data: newData,
    });
  }

  res.status(200).json({
    success: false,
    err: ec,
    message: "[api/getdata] stm32_dht11 GetData failed, error: " + ec,
  });
});

router.get("/isinit", (req: Request, res: Response) => {
  let ret = stm32_dht11.IsInit();
  if (ret === true) {
    return res.status(200).json({
      success: true,
      err: 0,
      message: "[api/isinit] stm32_dht11 already initialized",
    });
  }

  res.status(200).json({
    success: false,
    err: 0,
    message: "[api/isinit] stm32_dht11 not yet initialized",
  });
});

export default router;
