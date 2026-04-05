BUILD_DIR ?= build
CMAKE ?= cmake

.PHONY: configure build test run clean

configure:
	$(CMAKE) -S . -B $(BUILD_DIR)

build: configure
	$(CMAKE) --build $(BUILD_DIR) --config Release

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

run: build
	$(BUILD_DIR)/align_cli --input instances/Inst_0000010_7.adn --solver sol2 --show-alignment

clean:
	$(CMAKE) -E rm -rf $(BUILD_DIR)

