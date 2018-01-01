.PHONY: clean All

All:
	@echo "----------Building project:[ testapp - Debug ]----------"
	@cd "jasper" && "$(MAKE)" -f  "testapp.mk"
clean:
	@echo "----------Cleaning project:[ testapp - Debug ]----------"
	@cd "jasper" && "$(MAKE)" -f  "testapp.mk" clean
