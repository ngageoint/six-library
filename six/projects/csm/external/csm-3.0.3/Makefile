

HEADERS = csm.h Error.h Warning.h Version.h Isd.h BytestreamIsd.h NitfIsd.h Plugin.h Model.h GeometricModel.h RasterGM.h CorrelationModel.h FourParameterCorrelationModel.h LinearDecayCorrelationModel.h csmPointCloud.h PointCloudIsd.h PointCloudGM.h ModelIdentifier.h BundleGM.h Ellipsoid.h SettableEllipsoid.h

OBJS = Version.o Isd.o Plugin.o GeometricModel.o RasterGM.o CorrelationModel.o FourParameterCorrelationModel.o LinearDecayCorrelationModel.o csmPointCloud.o PointCloudIsd.o PointCloudGM.o ModelIdentifier.o BundleGM.o Ellipsoid.o SettableEllipsoid.o


LIBNAME=libcsmapi
LIBVERSION=3.0.3

DOXYGEN_OUT=/programs/origin/html/doxygen/csm3
DOXYGEN_FILTER=$(PWD)/scripts/doxygen-filter.pl

LIBRARY=$(LIBNAME).so.$(LIBVERSION)
LIBS=-lm -ldl

MKDIR=mkdir
CP=cp -f
LN=ln -s
TAR=tar

LD=$(CC)

# compute the "major" version from the library version
MAJORLIBVERSION=$(word 1,$(subst ., ,$(LIBVERSION)))
SONAME=$(LIBNAME).so.$(MAJORLIBVERSION)

%.o: %.cpp
	$(CC) -c $(COPTS) $< -o $@

%.o: %.cc
	$(CC) -c $(COPTS) $< -o $@

$(LIBRARY): $(OBJS)
	$(LD) $(COPTS) $(LDOPTS) $^ $(LIBS) -o $@

all: $(HEADERS) $(LIBRARY)

install::
	$(MKDIR) -p $(INSTDIR)/lib
	$(CP) $(LIBRARY) $(INSTDIR)/lib
	$(RM) $(INSTDIR)/lib/$(LIBNAME).so && $(LN) $(LIBRARY) $(INSTDIR)/lib/$(LIBNAME).so
	@if [ "$(LIBVERSION)" != "$(MAJORLIBVERSION)" ]; then $(RM) $(INSTDIR)/lib/$(LIBNAME).so.$(MAJORLIBVERSION) && $(LN) $(LIBRARY) $(INSTDIR)/lib/$(LIBNAME).so.$(MAJORLIBVERSION); fi
	$(MKDIR) -p $(INSTDIR)/include/csm
	$(CP) $(HEADERS) $(INSTDIR)/include/csm

clean::
	$(RM) $(OBJS) $(LIBRARY) *~

package::
	$(TAR) -czvf csm3.$(shell date '+%Y%m%d').tar.gz Makefile* $(HEADERS) $(OBJS:.o=.cpp) Doxyfile scripts/doxygen-filter.pl

DOXYFILE=$(INSTDIR)/include/Doxyfile

doxygen::
	cat Doxyfile > $(DOXYFILE)
	echo "OUTPUT_DIRECTORY       = $(DOXYGEN_OUT)" >> $(DOXYFILE)
	echo "INPUT_FILTER           = $(DOXYGEN_FILTER)" >> $(DOXYFILE)
	rm -rf $(DOXYGEN_OUT)
	cd $(dir $(DOXYFILE)) && doxygen

