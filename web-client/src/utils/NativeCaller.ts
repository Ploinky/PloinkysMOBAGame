class NativeCaller {
    public static async connect(ip: string) {
        (window as any).chrome.webview.hostObjects.launcher.Connect(ip);
    }

    public static async quit() {
        (window as any).chrome.webview.hostObjects.launcher.Quit();
    }

    public static async minimize() {
        (window as any).chrome.webview.hostObjects.launcher.Minimize();
    }
}

export { NativeCaller };