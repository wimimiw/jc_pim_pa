using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;
using System.Net.Sockets;
using System.Threading;

namespace JcMcuUpgrade
{
    //public class MyProgressBar : ProgressBar
    //{
    //    public MyProgressBar()
    //    {
    //        SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint, true);
    //    }

    //    protected override void OnPaint(PaintEventArgs e)
    //    {
    //        Rectangle rect = ClientRectangle;
    //        Graphics g = e.Graphics;

    //        ProgressBarRenderer.DrawHorizontalBar(g, rect);
    //        rect.Inflate(-3, -3);
    //        if (Value > 0)
    //        {
    //            var clip = new Rectangle(rect.X, rect.Y, (int)((float)Value / Maximum * rect.Width), rect.Height);
    //            ProgressBarRenderer.DrawHorizontalChunks(g, clip);
    //        }

    //        string text = Value + "%";
    //        using (var font = new Font(FontFamily.GenericSerif, 20))
    //        {
    //            SizeF sz = g.MeasureString(text, font);
    //            var location = new PointF(rect.Width / 2 - sz.Width / 2, rect.Height / 2 - sz.Height / 2 + 2);
    //            g.DrawString(text, font, Brushes.Red, location);
    //        }
    //    }
    //}

    public partial class Form1 : Form
    {       
        string binFile = "";
        string portName = "";
        Byte __addr = 0;
        ComOper __tComOper;

        public Form1()
        {
            InitializeComponent();
        }

        private void toolStripLabel2_Click(object sender, EventArgs e)
        {
            AboutBox1 ab1 = new AboutBox1();
            ab1.ShowDialog();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            //this.progressBar1.SendToBack();
            //this.progressBar1.BackColor = Color.Transparent;
            //this.label1.Parent = this.progressBar1;
            //this.label1.BackColor = Color.Transparent;
            //this.label1.BringToFront();     
            this.toolStripStatusLabel1.Text = "";
            this.cbbType.Items.AddRange(new string[] { "矩阵开关（以太网）", "矩阵开关（485）", "功放" });
            this.cbbCom.Items.AddRange(SerialPort.GetPortNames());
            this.Text = "等待中...";
        }

        private void cbbType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (this.cbbType.SelectedIndex == 0)
            {
                this.cbbCom.Enabled = false;
                this.tbIP.Enabled = true;
                this.tbPort.Enabled = true;
            }
            else if (this.cbbType.SelectedIndex == 1)
            {
                this.cbbCom.Enabled = true;
                this.tbIP.Enabled = false;
                this.tbPort.Enabled = false;
            }
            else if (this.cbbType.SelectedIndex == 2)
            {
                this.cbbCom.Enabled = true;
                this.tbIP.Enabled = false;
                this.tbPort.Enabled = false;
            }
        }

        void UpgradeTask()
        {
            try
            {
                this.Invoke(new MethodInvoker(delegate{                    
                    this.Text = "下载中...";
                }));

                Byte[] frame = new Byte[] { (Byte)(this.__addr), (Byte)'F', (Byte)'S', (Byte)'T', (Byte)'A' };
                Byte[] ack = new Byte[2] {0,0};

                if(__tComOper.ID == 1)
                    frame = new Byte[] { (Byte)(this.__addr), (Byte)'F', (Byte)'S', (Byte)'T', (Byte)'B' };

                __tComOper.write(frame, 0, frame.Length);
                __tComOper.read(ack,0,(int)ack.Length,1000);

                if (ack[1] != (Byte)'Y')
                {
                    __tComOper.close();
                    MessageBox.Show(null, "下载指示失败！请检查是否端口被占用。", "WARING", MessageBoxButtons.OK,MessageBoxIcon.Warning);
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
                if (__tComOper.ID == 0)
                {
                    __tComOper.close();
                    __tComOper.open(portName,115200,2);
                }

                for (int i = 0; i < 0x7000; i+= DOW_SECTION_LEN)
                {
                    txBuf[0] = this.__addr;
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
                        Array.Clear(ack,0,2);
                        __tComOper.write(txBuf, 0, 7 + cnt);
                        __tComOper.read(ack, 0, (int)ack.Length,1000);

                        if (ack[1] != (Byte)'Y' && j == 3)
                        {
                            __tComOper.close();
                            MessageBox.Show(null, "下载过程失败", "WARING", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                            this.Invoke(new MethodInvoker(delegate
                            {
                                this.Text = "等待中...";
                            }));
                            return;
                        }
                        else if (ack[1] == (Byte)'Y')
                        {
                            break;
                        }
                        else
                            continue;
                    }

                    this.Invoke(new MethodInvoker(delegate
                    {
                        this.progressBar1.Value = i;
                        this.toolStripStatusLabel1.Text = (i * 100 / this.progressBar1.Maximum).ToString()+"%";
                    }));

                    Thread.Sleep(50);
                }

                frame = new Byte[] { (Byte)(this.__addr), (Byte)'F', (Byte)'E', (Byte)'X', (Byte)'E' };

                Array.Clear(ack, 0, 2);
                __tComOper.write(frame, 0, frame.Length);

                if (__tComOper.ID == 0)
                {
                    __tComOper.read(ack, 0, (int)ack.Length, 3000);

                    if (ack[1] != (Byte)'Y')
                    {
                        MessageBox.Show(null, "下载升级失败", "WARING", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                    else
                    {
                        this.Invoke(new MethodInvoker(delegate
                        {
                            this.Text = "升级成功！";
                            this.button1.Enabled = true;
                            this.progressBar1.Value = this.progressBar1.Maximum;
                            this.progressBar1.ForeColor = Color.Gray;
                            this.toolStripStatusLabel1.Text = "100%";
                        }));
                    }
                }
                else if (__tComOper.ID == 1)
                {
                    Thread.Sleep(3000);

                    Byte[] eACK = new Byte[5];
                    frame = new Byte[] { (Byte)(this.__addr), (Byte)'F', (Byte)'C', (Byte)'H', (Byte)'K' };

                    __tComOper.close();
                    Thread.Sleep(2000);//必须大于此延时
                    __tComOper.open();
                    __tComOper.write(frame, 0, frame.Length);
                    __tComOper.read(eACK, 0, (int)eACK.Length, 1000);

                    Byte[] fsReadBuf = File.ReadAllBytes(this.binFile);
                    ushort chk = 0;

                    for (int i = 0; i < 0x7000; i++)
                    {
                        chk += fsReadBuf[i];
                    }

                    if ( Convert.ToString(chk,16).ToUpper() == Encoding.ASCII.GetString(eACK, 1, eACK.Length - 1))
                    {
                        this.Invoke(new MethodInvoker(delegate
                        {
                            this.Text = "升级成功！";
                            this.button1.Enabled = true;
                            this.progressBar1.Value = this.progressBar1.Maximum;
                            this.progressBar1.ForeColor = Color.Gray;
                            this.toolStripStatusLabel1.Text = "100%";
                        }));
                    }
                    else
                    {
                        MessageBox.Show(this, "下载升级失败", "WARING", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                }

                __tComOper.close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(null, ex.Message, "WARING", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (this.binFile == "")
            {
                MessageBox.Show(this, "请选择固件！", "WARING", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }            

            this.__addr = (Byte)this.nudAddr.Value;

            if (this.cbbType.SelectedIndex == 0)
            {
                __tComOper = new Ethernet();
                __tComOper.open(this.tbIP.Text, int.Parse(this.tbPort.Text));
            }
            else if (this.cbbType.SelectedIndex == 1)
            {
                if (this.cbbCom.Text.Equals("")) { MessageBox.Show(this, "请选择串口!", "WARNING", MessageBoxButtons.OK, MessageBoxIcon.Warning); return; };
                __tComOper = new Uart();
                __tComOper.open(this.cbbCom.Text, 19200, 2);
                portName = this.cbbCom.Text;
            }
            else if (this.cbbType.SelectedIndex == 2)
            {
                if (this.cbbCom.Text.Equals("")) { MessageBox.Show(this, "请选择串口!", "WARNING", MessageBoxButtons.OK, MessageBoxIcon.Warning); return; };
                __tComOper = new Uart();
                __tComOper.open(this.cbbCom.Text, 9600, 2);
                portName = this.cbbCom.Text;
            }

            Button bt = sender as Button;
            bt.Enabled = false;
            Thread thrd = new Thread(new ThreadStart(UpgradeTask));
            thrd.Start();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();

            ofd.Filter = "BIN file|*.bin";
            ofd.FileName = string.Empty;

            this.binFile = DialogResult.OK == ofd.ShowDialog() ? ofd.FileName : "";

            if (this.binFile != "")
            {
                Byte[] fsBuf = File.ReadAllBytes(this.binFile);
                ushort chk = 0;

                for (int i = 0; i < 0x7000; i++)
                {
                    chk += fsBuf[i];
                }

                this.lblInfo.Text = "文件路径:"+this.binFile+"\n\r";
                this.lblInfo.Text+= "文件信息:大小 = " + fsBuf.Length.ToString() + " KB  校验和 = 0x" + chk.ToString("X04");
            }
        }
    }
}
