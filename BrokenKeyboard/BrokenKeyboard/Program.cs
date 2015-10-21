using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace BrokenKeyboard
{
    class Program
    {
        static class ExitCodes
        {
            public static int Success = 0;
            public static int SyntaxError = 1;
        }

        static int Main(string[] args)
        {
            int nLines;
            if (!int.TryParse(Console.ReadLine(), out nLines))
            {
                Console.Error.WriteLine("First line must be an integer line count");
                return ExitCodes.SyntaxError;
            }

            var listPath = Path.Combine(
                Environment.ExpandEnvironmentVariables("%HOMEDRIVE%%HOMEPATH%"),
                "wordlist.txt"
                );

            var wordList = File.ReadAllLines(listPath);

            for (int i = 0; i < nLines; i++)
            {
                string input = Console.ReadLine();
                string pattern = "^[" + string.Join("", input.Distinct()) + "]+$";
                var matches = from word in wordList
                              where Regex.IsMatch(word, pattern)
                              select word;
                string topWord = matches.OrderByDescending(w => w.Length).FirstOrDefault();
                Console.WriteLine("{0} = {1}", input, topWord ?? "No Match");
            }

            return ExitCodes.Success;
        }
    }
}
