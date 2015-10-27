//https://www.reddit.com/r/dailyprogrammer/comments/3q9vpn/20151026_challenge_238_easy_consonants_and_vowels/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsonantsAndVowels
{
    class Program
    {
        static Dictionary<char, string> symbols = new Dictionary<char, string>()
        {
            {'c', "bcdfghjklmnpqrstvwxyz"},
            {'C', "BCDFGHJKLMNPQRSTVWXYZ"},
            {'v', "aeiou"},
            {'V', "AEIOU"}
        };

        static void Main(string[] args)
        {
            Random r = new Random();
            while (true)
            {
                try
                {
                    string pattern = Console.ReadLine();

                    string result = new string(
                        pattern.Select(c =>
                            symbols[c][r.Next(symbols[c].Length)]
                        ).ToArray()
                    );

                    Console.WriteLine(result);
                }
                catch (KeyNotFoundException)
                {
                    Console.Error.WriteLine("Invalid Character");
                }
            }
        }
    }
}
