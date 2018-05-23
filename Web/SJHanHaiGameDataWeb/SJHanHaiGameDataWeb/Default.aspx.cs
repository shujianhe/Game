using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace SJHanHaiGameDataWeb
{
    public partial class _Default : Page
    {
        TextBox TextB = null;
        protected void Page_Load(object sender, EventArgs e)
        {
            TextB = new TextBox();
            TextB.BackColor = System.Drawing.Color.Green;//ConsoleColor.Green;
            TextB.Width = 800;
            TextB.Height = 600;            
            this.AddedControl(TextB,1);
        }
    }
}