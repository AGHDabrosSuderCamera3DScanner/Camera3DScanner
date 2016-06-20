using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viewer
{
    public static class PointLoader
    {
        private static Point[] points;
        private static float maxX = float.NegativeInfinity, minX = float.PositiveInfinity;
        private static float maxY = float.NegativeInfinity, minY = float.PositiveInfinity;
        private static float maxZ = float.NegativeInfinity, minZ = float.PositiveInfinity;
        private static float x, y, z;

        public static Point[] loadFromFile(String filepath)
        {
            string[] pointsString = System.IO.File.ReadAllLines(@filepath);
            points = new Point[pointsString.Length];

            for (int i = 0; i < pointsString.Length; i++)
            {
                stringToFloatXYZ(pointsString[i]);
                points[i] = new Point(x, y, z);
                checkTranslationValues();
            }

            translatePoints();

            return points;
        }

        private static void translatePoints()
        {
            float xTranslation = (maxX + minX) / 2;
            float yTranslation = (maxY + minY) / 2;
            float zTranslation = (maxZ + minZ) / 2;
            if(xTranslation > 0){
                xTranslation *= -1;
            }
            if (yTranslation > 0) 
            {
                yTranslation *= -1;
            }
            if (zTranslation > 0)
            {
                zTranslation *= -1;
            }

            foreach(Point p in points){
                p.x += xTranslation;
                p.y += yTranslation;
                p.z += zTranslation;
            }
        }

        private static void stringToFloatXYZ(String xyzInStringFormat)
        {
            string[] xyz = xyzInStringFormat.Split(' ');
            try
            {
                x = float.Parse(xyz[0]);
                y = float.Parse(xyz[1]);
                z = float.Parse(xyz[2]);
            }
            catch
            {
                throw new FormatException();
            }
        }

        private static void checkTranslationValues()
        {
            if (maxX < x)
            {
                maxX = x;
            }
            if (maxY < y)
            {
                maxY = y;
            }
            if (maxZ < z)
            {
                maxZ = z;
            } 
            if (minX > x)
            {
                minX = x;
            }
            if (minY > y)
            {
                minY = y;
            }
            if (minZ > z)
            {
                minZ = z;
            }
        }
    }
}
