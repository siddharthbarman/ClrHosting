using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using System.Windows.Forms;

namespace testapp
{
	class Program
	{
		static void Main(string[] args)
		{
			Console.WriteLine(Assembly.GetEntryAssembly().FullName);
			MessageBox.Show("Hello World!");
		}
	}
}
