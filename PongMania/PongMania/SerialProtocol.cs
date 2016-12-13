using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PongMania
{
    class SerialProtocol
    {
        const byte flg = 0x10;
        const byte esc = 0x1b;
        const byte ack = 0x06;
        const byte nack = 0x15;
        const int max_frame_length = 4;
        System.IO.Ports.SerialPort sPort;
        

        public SerialProtocol(String portName, int baudRate, System.IO.Ports.Parity parity, int dataBits, System.IO.Ports.StopBits stopBits)
        {
            sPort = new System.IO.Ports.SerialPort(portName, baudRate, parity, dataBits, stopBits);
            sPort.Open();
            
        }

        private void package(Queue<byte[]> frame_queue, Queue<byte> byte_queue)
        {
            while (byte_queue.Count > max_frame_length)
            {
                byte[] frame = new byte[max_frame_length + 2];
                frame[0] = flg;
                frame[frame.Length - 1] = flg;

                for (int i = 0; i < max_frame_length; ++i)
                {
                    frame[1 + i] = byte_queue.Dequeue();
                }
                frame_queue.Enqueue(frame);
            }

            if (byte_queue.Count > 0)
            {
                int count = byte_queue.Count;
                byte[] frame = new byte[count + 2];
                frame[0] = flg;
                frame[frame.Length - 1] = flg;

                for (int i = 0; i < count; ++i)
                {
                    frame[1 + i] = byte_queue.Dequeue();
                }
                frame_queue.Enqueue(frame);
            }
            
        }

        private void escape(Queue<byte> byte_queue, byte[] msg)
        {
            foreach (byte b in msg)
            {
                if (b == flg || b == esc)
                {
                    byte_queue.Enqueue(esc);
                }
                byte_queue.Enqueue(b);
            }
        }

        public void send(byte[] msg)
        {
            Queue<byte> byte_queue = new Queue<byte>();
            Queue<byte[]> frame_queue = new Queue<byte[]>();
            escape(byte_queue, msg);
            package(frame_queue, byte_queue);

            while(frame_queue.Count != 0)
            {
                int tries = 0;
                bool is_acked = false;
                byte[] frame = frame_queue.Dequeue();

                while (!is_acked && tries < 5)
                {
                    sPort.Write(frame, 0, frame.Length);
                    Console.WriteLine("frame sent");
                    byte buff = (byte) sPort.ReadByte();
                    Console.WriteLine(buff);
                    if (buff == ack)
                    {
                        is_acked = true;
                    }
                    else
                    {
                        ++tries;
                    }
                } 
            }

        }
    }
}
