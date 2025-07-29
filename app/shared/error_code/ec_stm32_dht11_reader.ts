// Must follow the EC in STM32DHT11Reader.h
export enum EC {
  None = 0,
  Ok,
  STLinkPortNotFound,
  OpenComPortFailed,
  GetCommStateFailed,
  SetCommStateFailed,
  ComPortNotInitialized,
  WriteToStm32Failed,
  ReadFromStm32Failed,
  ChecksumFailed
}

export function PrintErrorCodeString(ec: EC): string {
  switch (ec) {
    case EC.None:                  return "EC::None"
    case EC.Ok:                    return "EC::Ok"
    case EC.STLinkPortNotFound:    return "EC::STLinkPortNotFound";
    case EC.OpenComPortFailed:     return "EC::OpenComPortFailed";
    case EC.GetCommStateFailed:    return "EC::GetCommStateFailed";
    case EC.SetCommStateFailed:    return "EC::SetCommStateFailed";
    case EC.ComPortNotInitialized: return "EC::ComPortNotInitialized";
    case EC.WriteToStm32Failed:    return "EC::WriteToStm32Failed";
    case EC.ReadFromStm32Failed:   return "EC::ReadFromStm32Failed";
    case EC.ChecksumFailed:        return "EC::ChecksumFailed";
    default:                       return "Undefined error code";
  }
}