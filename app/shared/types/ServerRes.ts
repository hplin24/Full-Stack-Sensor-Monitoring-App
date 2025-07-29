export interface ServerRes {
  success: boolean;
  err: number;
  message: string;
  data?: { temp: number; humidity: number };
}