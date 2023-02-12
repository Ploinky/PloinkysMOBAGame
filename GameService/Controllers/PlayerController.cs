using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using System.Net;

namespace GameService.Controllers
{
    [Route("players")]
    public class PlayerController : Controller
    {
        private static readonly List<Player> staticPlayers = new List<Player>
        {
            new Player("Ploinky"),
        };

        [HttpGet]
        public IEnumerable<Player> Get()
        {
            return staticPlayers;
        }

        [HttpPost]
        public StatusCodeResult Post([FromBody] Player player)
        {
            staticPlayers.Add(player);
            return StatusCode(StatusCodes.Status201Created);
        }
    }
}
