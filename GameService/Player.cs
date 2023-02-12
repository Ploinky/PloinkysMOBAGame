namespace GameService
{
    public class Player
    {
        public Player() : this(null) { }

        public Player(String? _name)
        {
            PlayerName = _name;
        }

        public String? PlayerName { get; set; }
    }
}
