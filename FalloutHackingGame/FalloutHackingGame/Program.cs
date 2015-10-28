// https://www.reddit.com/r/dailyprogrammer/comments/3qjnil/20151028_challenge_238_intermediate_fallout/

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace FalloutHackingGame
{
    static class Program
    {
        static Dictionary<int, int> difToLength = new Dictionary<int, int>
        {
            {1, 4},
            {2, 5},
            {3, 6},
            {4, 7},
            {5, 8}
        };

        static Dictionary<int, int> difToCount = new Dictionary<int, int>
        {
            {1, 5},
            {2, 10},
            {3, 10},
            {4, 15},
            {5, 15}
        };

        static void Main(string[] args)
        {
            var wordList = File.ReadAllLines("enable1.txt");
            while (true)
            {
                int difficulty;
                do
                    Console.Write("Difficulty (1-5)? ");
                while (!int.TryParse(Console.ReadLine(), out difficulty) || difficulty < 1 || difficulty > 5);
                int length = difToLength[difficulty];
                int count = difToCount[difficulty];

                var words = wordList
                    .Where(w => w.Length == length)
                    .Shuffle()
                    .Take(count)
                    .Select(w => w.ToUpperInvariant())
                    .ToList(); // TIL that IEnumerable uses deferred execution.

                foreach (string word in words)
                    Console.WriteLine(word);

                string answer = words.ElementAt(new Random().Next(0, count));

                bool victory = false;
                for (int guesses = 4; guesses > 0; guesses--)
                {
                    Console.Write($"Guess ({guesses} left)? ");
                    string guess = Console.ReadLine().ToUpperInvariant();

                    int similarity = 0;
                    for (int i = 0; i < guess.Length && i < length; i++)
                    {
                        if (guess[i] == answer[i])
                            similarity++;
                    }

                    Console.WriteLine($"{similarity}/{length} correct");

                    if (similarity == length)
                    {
                        victory = true;
                        break;
                    }
                }
                if (victory)
                    Console.WriteLine("You win!");
                else
                    Console.WriteLine($"Sorry, you lose. The answer was {answer}.");
            }
        }

        /// <summary>
        /// Returns a shuffled copy of an <c>IEnumerable</c> using the Fisher-Yates algorithm
        /// </summary>
        /// <typeparam name="T">The element type</typeparam>
        /// <param name="list">The list to shuffle</param>
        /// <returns>A shuffled copy of the original list</returns>
        static IEnumerable<T> Shuffle<T>(this IEnumerable<T> list)
        {
            Random r = new Random();
            List<T> copy = new List<T>(list);
            int count = copy.Count();
            for (int i = 0; i < list.Count(); i++)
            {
                int idx = r.Next(i, count);

                yield return copy[idx];

                // Don't need to bother swapping copy[idx] into copy[i] since
                // it isn't used again anyway.
                copy[idx] = copy[i];

            }
        }
    }
}
