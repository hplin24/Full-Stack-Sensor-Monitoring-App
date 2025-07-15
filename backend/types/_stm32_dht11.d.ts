declare module 'stm32_dht11.node' {
  export function Initialize(): number;
  export function IsInit(): boolean;
  export function ReInit(): number;
  export function GetData(data: { temp: number; rh: number }): number;
}