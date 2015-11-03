// https://www.reddit.com/r/dailyprogrammer/comments/3r7wxz/20151102_challenge_239_easy_a_game_of_threes/

using System;

namespace A_Game_of_Threes
{
    class Program
    {
        static void Main(string[] args)
        {
            uint n;
            if (args.Length < 1 || !uint.TryParse(args[0], out n))
            {
                Console.WriteLine($@"Usage: ""{AppDomain.CurrentDomain.FriendlyName}"" <uint>");
                return;
            }

            int[] modifiers = { 0, -1, 1 };

            while (n != 1)
            {
                Console.WriteLine($"{n} {modifiers[n % 3]}");
                n = (n + 1) / 3; // NOTE: don't actually need to use the modifier... just truncate
            }

            Console.WriteLine("1");
        }
    }
}
