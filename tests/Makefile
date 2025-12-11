# Makefile for desktop testing of Arduino code

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I. -I..
LDFLAGS =

# Test executables
TESTS = test_audio_processor test_motor_controller

# Mock objects
MOCK_OBJS = mock_arduino.o

all: $(TESTS)

test_audio_processor: test_audio_processor.cpp $(MOCK_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(MOCK_OBJS) $(LDFLAGS)

test_motor_controller: test_motor_controller.cpp $(MOCK_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(MOCK_OBJS) $(LDFLAGS)

mock_arduino.o: mock_arduino.cpp mock_arduino.h
	$(CXX) $(CXXFLAGS) -c mock_arduino.cpp

run: all
	@echo "\n========================================="
	@echo "Running all tests..."
	@echo "=========================================\n"
	@./test_audio_processor
	@./test_motor_controller
	@echo "\n========================================="
	@echo "All tests completed!"
	@echo "=========================================\n"

clean:
	rm -f $(TESTS) $(MOCK_OBJS)

.PHONY: all run clean




