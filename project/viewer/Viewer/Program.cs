using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace Viewer
{
    public static class Program
    {
        private static string filepath;
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Console.WriteLine("Usage: [filepath]");
                Environment.Exit(-1);
            }
            filepath = args[0];

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            
            Application.Run(new SharpGLForm(readFileForPoints()));
        }

        private static Point[] readFileForPoints()
        {
            Point[] points = null;
            try
            {
                string[] pointsString = System.IO.File.ReadAllLines(@filepath);
                points = new Point[pointsString.Length];
                for (int i = 0; i < pointsString.Length; i++)
                {
                    points[i] = new Point(pointsString[i]);
                }
            }
            catch
            {
                Environment.Exit(-1);
            }
            return points;
        }
    }
}
