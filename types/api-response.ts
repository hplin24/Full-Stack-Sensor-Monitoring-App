
export interface ApiResponse {
  success: boolean;
  err: number;
  message: string;
  data?: { temp: number, rh: number };
}