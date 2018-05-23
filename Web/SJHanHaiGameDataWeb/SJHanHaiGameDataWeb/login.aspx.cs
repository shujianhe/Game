using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.Script.Serialization;
using System.Web.Script.Services;
namespace SJHanHaiGameDataWeb
{
    public partial class login : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            Dictionary<string, object> dic = new Dictionary<string, object>();
            Func<bool> back = delegate()
            {
                logic.User.Login(Request, ref dic);
                return true;
            };
            SJUIPage.OnPageLoad(this,ref dic,back);
        }
    }
}