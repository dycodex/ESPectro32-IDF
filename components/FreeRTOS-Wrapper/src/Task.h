/*
 * Task.h
 *
 *  Created on: Mar 4, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_TASK_H_
#define COMPONENTS_CPP_UTILS_TASK_H_
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>
/**
 * @brief Encapsulate a runnable task.
 *
 * This class is designed to be subclassed with the method:
 *
 * @code{.cpp}
 * void runAsync(void *data) { ... }
 * @endcode
 *
 * For example:
 *
 * @code{.cpp}
 * class BackgroundTask : public Task {
 *    void runAsync(void *data) {
 *       // Do something
 *    }
 * };
 * @endcode
 *
 * implemented.
 */
class Task {
public:
	Task(std::string taskName="Task", uint16_t stackSize=2048, UBaseType_t priority = 5);
	Task(BaseType_t coreId, std::string taskName="Task", uint16_t stackSize=2048, UBaseType_t priority = 5);
	virtual ~Task();
	void setStackSize(uint16_t stackSize);
	void setPriority(UBaseType_t priority);
	void setTaskName(std::string &taskN);
	void start(void *taskData=nullptr);
	void stop();
	/**
	 * @brief Body of the task to execute.
	 *
	 * This function must be implemented in the subclass that represents the actual task to run.
	 * When a task is started by calling start(), this is the code that is executed in the
	 * newly created task.
	 *
	 * @param [in] data The data passed in to the newly started task.
	 */
	virtual void runAsync(void *data) = 0; // Make run pure virtual
	void delay(int ms);

private:
	xTaskHandle handle;
	void *taskData;
	static void runTask(void *data);
	std::string taskName;
	uint16_t stackSize;
	UBaseType_t priority = 5;

	BaseType_t coreId = -1;
};

#endif /* COMPONENTS_CPP_UTILS_TASK_H_ */
