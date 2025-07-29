declare module 'stm32_dht11_reader' {
  export function Init(): number;
  export function ReInit(): number;
  export function IsInit(): boolean;
  export function UpdateData(): number;
  export function GetTemperature(): number;
  export function GetHumidity(): number;
}
