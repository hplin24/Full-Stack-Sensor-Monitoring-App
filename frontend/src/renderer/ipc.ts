//import { ipcRenderer } from "electron";

export function getTempAndHum(): { temp: number; hum: number } {
    //ipcRenderer.sendSync('getHidDevice', '');
    return {
        temp: 29,
        hum: 50
    }
}
