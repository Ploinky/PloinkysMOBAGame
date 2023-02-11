class PlayerService {
    public static async login(playerName: string, pwd: string): Promise<boolean> {
        if(pwd === "123") {
            return true;
        }

        return false;
    }
}

export { PlayerService };