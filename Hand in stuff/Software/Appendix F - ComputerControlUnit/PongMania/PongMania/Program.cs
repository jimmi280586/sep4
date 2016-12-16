using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PongMania
{
    class Program
    {
        static void Main(string[] args)
        {
            SerialProtocol sp = new SerialProtocol("COM3", 115200, System.IO.Ports.Parity.None, 8, System.IO.Ports.StopBits.One);

            while (true)
            {
                ConsoleKeyInfo key = Console.ReadKey();

                if (key.Key.Equals(ConsoleKey.UpArrow))
                {
                    byte[] msg = {0x77};
                    sp.send(msg);
                }
                else if (key.Key.Equals(ConsoleKey.DownArrow))
                {
                    byte[] msg = { 0x73 };
                    sp.send(msg);
                }
                System.Threading.Thread.Sleep(20);
            }
        }
    }
}
