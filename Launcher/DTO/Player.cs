using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Launcher.DTO
{
    public class Player
    {
        public string? PlayerName { get; set; }

        public Player() : this(null) { }

        public Player(string? _name)
        {
            PlayerName = _name;
        }
    }
}
