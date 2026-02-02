using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Avalonia;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config; 
public class CheatWindowConfig : BaseWindowConfig<CheatWindowConfig> {
	public bool DisableAllCheats { get; set; } = false;
	public List<int> ColumnWidths { get; set; } = new();
}
