using Nexen.Interop;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config; 
public class MovieRecordConfig : BaseConfig<MovieRecordConfig> {
	[Reactive] public RecordMovieFrom RecordFrom { get; set; } = RecordMovieFrom.CurrentState;
	[Reactive] public string Author { get; set; } = "";
	[Reactive] public string Description { get; set; } = "";
}
