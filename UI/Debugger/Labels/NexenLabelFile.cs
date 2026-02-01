using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Nexen.Config;
using Nexen.Debugger.Labels;
using Nexen.Utilities;
using Nexen.Windows;

namespace Nexen.Debugger.Labels {
	public class NexenLabelFile {
		public static void Import(string path, bool showResult) {
			List<CodeLabel> labels = new(1000);

			int errorCount = 0;
			foreach (string row in File.ReadAllLines(path, Encoding.UTF8)) {
				CodeLabel? label = CodeLabel.FromString(row);
				if (label is null) {
					errorCount++;
				} else {
					if (ConfigManager.Config.Debug.Integration.IsMemoryTypeImportEnabled(label.MemoryType)) {
						if (label.Label.Length > 0 || ConfigManager.Config.Debug.Integration.ImportComments) {
							labels.Add(label);
						}
					}
				}
			}

			LabelManager.SetLabels(labels);

			if (showResult) {
				NexenMsgBox.Show(null, errorCount == 0 ? "ImportLabels" : "ImportLabelsWithErrors", MessageBoxButtons.OK, MessageBoxIcon.Question, labels.Count.ToString(), errorCount.ToString());
			}
		}

		public static void Export(string path) {
			List<CodeLabel> labels = LabelManager.GetAllLabels();

			labels.Sort((CodeLabel a, CodeLabel b) => {
				int result = a.MemoryType.CompareTo(b.MemoryType);
				if (result == 0) {
					return a.Address.CompareTo(b.Address);
				} else {
					return result;
				}
			});

			StringBuilder sb = new StringBuilder();
			foreach (CodeLabel label in labels) {
				sb.Append(label.ToString() + "\n");
			}

			FileHelper.WriteAllText(path, sb.ToString(), Encoding.UTF8);
		}
	}
}
