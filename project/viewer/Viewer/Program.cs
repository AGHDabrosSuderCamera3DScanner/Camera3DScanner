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
            
            Application.Run(new SharpGLForm(PointLoader.loadFromFile(filepath)));
        }
    }
}
