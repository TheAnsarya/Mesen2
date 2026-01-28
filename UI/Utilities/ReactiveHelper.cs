using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Reflection;
using ReactiveUI;
using ReactiveUI.Fody.Helpers;

namespace Mesen.Utilities {
	public static class ReactiveHelper {
		public static IDisposable RegisterRecursiveObserver(ReactiveObject target, PropertyChangedEventHandler handler) {
			Dictionary<string, ReactiveObject> observableObjects = new();
			Dictionary<string, PropertyInfo> props = new();

			foreach (PropertyInfo prop in target.GetType().GetProperties(BindingFlags.Instance | BindingFlags.Public)) {
				if (prop.GetCustomAttribute<ReactiveAttribute>() != null) {
					object? value = prop.GetValue(target);
					if (value is ReactiveObject propValue) {
						observableObjects[prop.Name] = propValue;
						props[prop.Name] = prop;
						ReactiveHelper.RegisterRecursiveObserver(propValue, handler);
					} else if (value is IList list) {
						foreach (object listValue in list) {
							if (listValue is ReactiveObject reactiveListValue) {
								ReactiveHelper.RegisterRecursiveObserver(reactiveListValue, handler);
							}
						}
					}
				}
			}

			target.PropertyChanged += (s, e) => {
				handler(s, e);

				//Reset change handlers if an object is replaced with another object
				if (e.PropertyName != null && observableObjects.TryGetValue(e.PropertyName, out ReactiveObject? obj)) {
					//Remove handlers on the old object
					ReactiveHelper.UnregisterRecursiveObserver(obj, handler);

					if (props.TryGetValue(e.PropertyName, out PropertyInfo? prop)) {
						object? value = prop.GetValue(target);
						if (value is ReactiveObject propValue) {
							observableObjects[prop.Name] = propValue;

							//Register change handlers on the new object
							ReactiveHelper.RegisterRecursiveObserver(propValue, handler);
						}
					}
				}
			};

			return new RecursiveObserver(target, handler);
		}

		public static void UnregisterRecursiveObserver(ReactiveObject target, PropertyChangedEventHandler handler) {
			foreach (PropertyInfo prop in target.GetType().GetProperties(BindingFlags.Instance | BindingFlags.Public)) {
				if (prop.GetCustomAttribute<ReactiveAttribute>() != null) {
					object? value = prop.GetValue(target);
					if (value is ReactiveObject propValue) {
						ReactiveHelper.UnregisterRecursiveObserver(propValue, handler);
					} else if (value is IList list) {
						foreach (object listValue in list) {
							if (listValue is ReactiveObject reactiveListValue) {
								ReactiveHelper.UnregisterRecursiveObserver(reactiveListValue, handler);
							}
						}
					}
				}
			}

			target.PropertyChanged -= handler;
		}
	}

	public class RecursiveObserver : IDisposable {
		private ReactiveObject _target;
		private PropertyChangedEventHandler _handler;

		public RecursiveObserver(ReactiveObject target, PropertyChangedEventHandler handler) {
			_target = target;
			_handler = handler;
		}

		public void Dispose() {
			ReactiveHelper.UnregisterRecursiveObserver(_target, _handler);
		}
	}
}
