.PHONY: clean All

All:
	@echo "----------Building project:[ testapp - Debug ]----------"
	@cd "testapp" && "$(MAKE)" -f  "testapp.mk"
clean:
	@echo "----------Cleaning project:[ testapp - Debug ]----------"
	@cd "testapp" && "$(MAKE)" -f  "testapp.mk" clean
