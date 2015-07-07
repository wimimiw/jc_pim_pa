using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;
using System.Threading;

namespace JcMcuUpgrade
{
    public class MyProgressBar : ProgressBar
    {
        public MyProgressBar()
        {
            SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint, true);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            Rectangle rect = ClientRectangle;
            Graphics g = e.Graphics;

            ProgressBarRenderer.DrawHorizontalBar(g, rect);
            rect.Inflate(-3, -3);
            if (Value > 0)
            {
                var clip = new Rectangle(rect.X, rect.Y, (int)((float)Value / Maximum * rect.Width), rect.Height);
                ProgressBarRenderer.DrawHorizontalChunks(g, clip);
            }

            string text = Value + "%";
            using (var font = new Font(FontFamily.GenericSerif, 20))
            {
                SizeF sz = g.MeasureString(text, font);
                var location = new PointF(rect.Width / 2 - sz.Width / 2, rect.Height / 2 - sz.Height / 2 + 2);
                g.DrawString(text, font, Brushes.Red, location);
            }
        }
    }

    public partial class Form1 : Form
    {       
        string binFile = "";
        ManualResetEvent mreCom = new ManualResetEvent(false);

        public Form1()
        {
            InitializeComponent();
        }

        private void toolStripLabel2_Click(object sender, EventArgs e)
        {
            AboutBox1 ab1 = new AboutBox1();
            ab1.ShowDialog();
        }

        private void toolStripLabel1_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();

            ofd.Filter = "BIN file|*.bin";
            ofd.FileName = string.Empty;

            this.binFile = DialogResult.OK == ofd.ShowDialog() ? ofd.FileName : "";

            if (this.binFile != "")
            {                
                toolStripLabel3.Enabled = true;
                this.toolStripStatusLabel1.Text = "固件文件：" +　this.binFile;

                Byte[] fsBuf = File.ReadAllBytes(this.binFile);
                ushort chk = 0;

                for (int i = 0; i < 0x7000; i++)
                {
                    chk += fsBuf[i];
                }

                this.toolStripLabel6.Text = "Size : "+fsBuf.Length.ToString()+" KB  CheckSum : 0x" + chk.ToString("X04");
            }
        }

        private void toolStripLabel3_Click(object sender, EventArgs e)
        {
            if (this.toolStripComboBox1.Text.Equals(""))
            {
                MessageBox.Show(this,"请选择串口","WARNING",MessageBoxButtons.OK,MessageBoxIcon.Warning);
                return;
            }

            Thread thrd = new Thread(new ThreadStart(UpgradeTask));
            thrd.Start();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            //this.progressBar1.SendToBack();
            //this.progressBar1.BackColor = Color.Transparent;
            //this.label1.Parent = this.progressBar1;
            //this.label1.BackColor = Color.Transparent;
            //this.label1.BringToFront();     
            this.toolStripStatusLabel1.Text = "";
            this.toolStripLabel6.Text = "";
            this.toolStripComboBox1.Items.AddRange(SerialPort.GetPortNames());
            this.Text = "等待中...";
        }

        void UpgradeTask()
        {
            try
            {
                string portName = "";
                this.Invoke(new MethodInvoker(delegate{
                    portName = this.toolStripComboBox1.Text;
                    this.Text = "下载中...";
                }));

                SerialPort sp = new SerialPort();

                sp.DataReceived += sp_DataReceived;
                sp.PortName = portName;
                sp.BaudRate = 9600;
                sp.ReceivedBytesThreshold = 2;

                char[] frame = new char[] { (char)(0), 'F', 'S', 'T', 'A' };
                sp.Open();

                mreCom.Reset();

                sp.Write(frame, 0, frame.Length);

                if (!mreCom.WaitOne(1000))
                {
                    sp.Close();
                    MessageBox.Show(null, "下载指示失败！", "WARING", MessageBoxButtons.OK,MessageBoxIcon.Warning);
                    this.Invoke(new MethodInvoker(delegate
                    {
                        this.Text = "等待中...";
                    }));
                    return;
                }

                Thread.Sleep(1000);

                const int DOW_SECTION_LEN = 200;

                Byte[] fsBuf = File.ReadAllBytes(this.binFile);
                Byte[] txBuf = new Byte[DOW_SECTION_LEN+7];

                this.Invoke(new MethodInvoker(delegate
                {
                    this.progressBar1.Value = 0;                    
                    this.progressBar1.Maximum = fsBuf.Length;
                }));

                //将波特率调整为115200
                sp.BaudRate = 115200;

                for (int i = 0; i < 0x7000; i+= DOW_SECTION_LEN)
                {                    
                    txBuf[0] = 0;
                    txBuf[1] = (Byte)('F');
                    txBuf[2] = (Byte)('D');
                    txBuf[3] = (Byte)('O');
                    txBuf[4] = (Byte)('W');

                    Byte chk = 0;
                    Byte cnt = (fsBuf.Length - i) >= 200 ? (Byte)200 : (Byte)(fsBuf.Length - i);

                    txBuf[5] = cnt;

                    for (int j = 0; j < cnt; j++)
			        {
                        txBuf[6 + j] = fsBuf[i + j];
                        chk ^= txBuf[6 + j];
			        }

                    txBuf[5 + cnt + 1] = chk;

                    for (int j = 0;true; j++)
                    {
                        mreCom.Reset();

                        sp.Write(txBuf, 0, 7 + cnt);

                        if (!mreCom.WaitOne(1000) && j == 3)
                        {
                            sp.Close();
                            MessageBox.Show(null, "下载过程失败", "WARING", MessageBoxButtons.OK,MessageBoxIcon.Warning);
                            this.Invoke(new MethodInvoker(delegate
                            {
                                this.Text = "等待中...";                                
                            }));
                            return;
                        }
                        else
                            break;
                    }

                    this.Invoke(new MethodInvoker(delegate
                    {
                        this.progressBar1.Value = i;
                    }));

                    Thread.Sleep(50);
                }

                frame = new char[] { (char)(0), 'F', 'E', 'X', 'E' };

                mreCom.Reset();

                sp.Write(frame, 0, frame.Length);                          

                if (!mreCom.WaitOne(3000))
                {                    
                    MessageBox.Show(null, "下载升级失败", "WARING", MessageBoxButtons.OK,MessageBoxIcon.Warning);                    
                }

                sp.Close();
                sp.Dispose();

                this.Invoke(new MethodInvoker(delegate
                {
                    this.Text = "升级成功！";
                    this.progressBar1.Value = this.progressBar1.Maximum;
                    this.progressBar1.ForeColor = Color.Gray;
                }));
            }
            catch (Exception ex)
            {
                MessageBox.Show(null, ex.Message, "WARING", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        void sp_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort sp = sender as SerialPort;
            Byte[] rdBuf = new Byte[10];

            sp.Read(rdBuf, 0, 2);

            if (rdBuf[1] == (Byte)('Y'))
            {
                mreCom.Set();
            }
            //throw new NotImplementedException();
        }
    }
}
