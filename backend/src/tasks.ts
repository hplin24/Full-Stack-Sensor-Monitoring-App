import { Router, Request, Response } from "express";
const dht11Reader = require('bindings')('DHT11Reader');

const router = Router();

router.get('/', (req: Request, res: Response) => {
  res.send('api server homepage');
});

router.get('/status', (req: Request, res: Response) => {
  res.send('good');
});

router.get('/initcomport', (req: Request, res: Response) => {
  let err = dht11Reader.InitComPort();
  res.send(err === 9 ? 'ok' : err);
});

router.get('/getdata', (req: Request, res: Response) => {
  const data: { temp: number; rh: number; } = {
    temp: 0,
    rh: 0
  };
  let err = dht11Reader.RetrieveData(data);
  if (err != 9)
    res.send('error: ' + err);
 
  res.send(data);    

});

router.get('/iscomportok', (req: Request, res: Response) => {
  res.send(dht11Reader.IsComPortOk());
});

export default router;