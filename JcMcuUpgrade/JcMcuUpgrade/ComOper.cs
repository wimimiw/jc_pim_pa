using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.Net.Sockets;
using System.Diagnostics;
using System.Threading;

namespace JcMcuUpgrade
{
    public abstract class ComOper
    {
        protected int id;
        public abstract int ID { get; set; }
        public abstract void open();
        public abstract void open(string ip, int port);
        public abstract void open(string com, int baud, int Threshold);
        public abstract int read(Byte[] buffer, int offset, int count,int timeout);
        public abstract int write(Byte[] buffer, int offset, int count);
        public abstract void close();
    }

    public class Uart : ComOper
    {
        string __port;
        int __baud;
        int __threod;
        SerialPort __Uart;
        ManualResetEvent mreCom = new ManualResetEvent(false);

        public override int ID
        {
            get
            {
                return 0;
                //throw new NotImplementedException();
            }
            set
            {
                //throw new NotImplementedException();
            }
        }

        public override void open()
        {
            this.open(__port, __baud, __threod);
        }
        public override void open(string ip, int port) { }
        public override void open(string com, int baud, int Threshold)
        {
            if (__Uart == null)
            {
                __Uart = new SerialPort();
            }

            __Uart.DataReceived += __Uart_DataReceived;
            __Uart.ReceivedBytesThreshold = Threshold;
            __Uart.PortName = com;
            __Uart.BaudRate = baud;
            __Uart.Open();

            __baud = baud;
            __port = com;
            __threod = Threshold;
        }

        void __Uart_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            mreCom.Set();
            //throw new NotImplementedException();
        }

        public override int read(Byte[] buffer, int offset, int count, int timeout)
        {
            int result = 0;

            try
            {
                if (mreCom.WaitOne(timeout))
                {
                    if (__Uart != null && __Uart.IsOpen)
                        result = __Uart.Read(buffer, offset, count);
                }
                else
                {
                    result = -1;
                }
            }
            catch
            {
                result = -1;
            }

            return result;
        }

        public override int write(Byte[] buffer, int offset, int count)
        {
            int result = 0;

            mreCom.Reset();

            try
            {
                if (__Uart != null && __Uart.IsOpen)
                    __Uart.Write(buffer, offset, count);
            }
            catch
            {
                result = -1;
            }

            return result;
        }

        public override void close()
        {
            if (__Uart != null)
                __Uart.Close();
        }
    }

    public class Ethernet : ComOper
    {
        string __ip;
        int __port;
        TcpClient __tcpClient;

        public override int ID
        {
            get
            {
                return 1;
                //throw new NotImplementedException();
            }
            set
            {
                //throw new NotImplementedException();
            }
        }

        public override void open()
        {
            this.open(__ip, __port);
        }

        public override void open(string ip, int port)
        {
            if (__tcpClient == null) __tcpClient = new TcpClient();

            this.__ip = ip;
            this.__port = port;

            try
            {
                __tcpClient.Connect(ip, port);
            }
            catch (Exception ex)
            {
                //System.Windows.Forms.MessageBox(ex.Message);
            }
        }

        public override void open(string com, int baud, int Threshold) 
        {
 
        }

        public override int read(Byte[] buffer, int offset, int count,int timeout)
        {
            int result = 0;

            try
            {
                if (__tcpClient != null && __tcpClient.Connected)
                {
                    __tcpClient.ReceiveTimeout = timeout;
                    result = __tcpClient.GetStream().Read(buffer, offset, count);
                }
            }
            catch
            {
                result = -1;
            }

            return result;
        }

        public override int write(Byte[] buffer, int offset, int count)
        {
            int result = 0;

            try
            {
                if (__tcpClient != null && __tcpClient.Connected)
                    __tcpClient.GetStream().Write(buffer, offset, count);
            }
            catch
            {
                result = -1;
            }

            return result;
        }
        public override void close()
        {
            if (__tcpClient != null)
            {
                __tcpClient.Close();
                __tcpClient = null;
            }
        }
    }
}
