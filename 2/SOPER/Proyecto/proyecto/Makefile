SUBDIRS=src

# BOLD=\e[1m
# NC=\e[0m

all:

%:
	@$(foreach dir, $(SUBDIRS), echo -e "$(BOLD)Building $(dir:/=)...$(NC)" && $(MAKE) -C $(dir) $@ &&) :

