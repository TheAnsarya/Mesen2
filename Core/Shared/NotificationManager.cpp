#include "pch.h"
#include <algorithm>
#include "Shared/NotificationManager.h"

void NotificationManager::RegisterNotificationListener(shared_ptr<INotificationListener> notificationListener) {
	auto lock = _lock.AcquireSafe();

	// Cleanup expired listeners on registration (rare) instead of every SendNotification (per-frame)
	CleanupNotificationListeners();

	for (weak_ptr<INotificationListener> listener : _listeners) {
		if (listener.lock() == notificationListener) {
			// This listener is already registered, do nothing
			return;
		}
	}

	_listeners.push_back(notificationListener);
}

void NotificationManager::CleanupNotificationListeners() {
	auto lock = _lock.AcquireSafe();

	// Remove expired listeners
	_listeners.erase(
	    std::remove_if(
	        _listeners.begin(),
	        _listeners.end(),
	        [](weak_ptr<INotificationListener> ptr) { return ptr.expired(); }),
	    _listeners.end());
}

void NotificationManager::SendNotification(ConsoleNotificationType type, void* parameter) {
	// Iterate under lock — ProcessNotification is fast and non-blocking.
	// Avoids per-frame vector copy + cleanup scan overhead.
	auto lock = _lock.AcquireSafe();
	for (size_t i = 0; i < _listeners.size(); i++) {
		shared_ptr<INotificationListener> listener = _listeners[i].lock();
		if (listener) {
			listener->ProcessNotification(type, parameter);
		}
	}
}
