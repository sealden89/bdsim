PYMADX = ./pymadx/
PYMAD8 = ./pymad8/
PYTRANSPORT = ./pytransport/
PYBDSIM = ./pybdsim/

all:
	$(MAKE) develop -C $(PYTRANSPORT)
	$(MAKE) develop -C $(PYMAD8)
	$(MAKE) develop -C $(PYMADX)
	$(MAKE) develop -C $(PYBDSIM)

venv:
	$(MAKE) venv -C $(PYTRANSPORT)
	$(MAKE) venv -C $(PYMAD8)
	$(MAKE) venv -C $(PYMADX)
	$(MAKE) venv -C $(PYBDSIM)

venv-shared:
	$(MAKE) venv-shared -C $(PYTRANSPORT)
	$(MAKE) venv-shared -C $(PYMAD8)
	$(MAKE) venv-shared -C $(PYMADX)
	$(MAKE) venv-shared -C $(PYBDSIM)

.DEFAULT_GOAL := all
