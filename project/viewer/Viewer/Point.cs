using SharpGL;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viewer
{
    public class Point
    {
        public float x;
        public float y;
        public float z;

        public float r = 1;
        public float g = 1;
        public float b = 1;

        public Point(float x, float y, float z)
        {
            this.x = x; this.y = y; this.z = z;
        }

        public void draw(OpenGL gl)
        {
            gl.PointSize(2);
            gl.Begin(OpenGL.GL_POINTS);
            gl.Color(r, g, b);
            gl.Vertex(x, y, z);
            gl.End();
        }

        public void setColor(float r, float g, float b)
        {
            this.r = r; this.g = g; this.b = b;
        }

        public override string ToString()
        {
            return "Point " + x + " " + y + " " + z;
        }
    }
}
