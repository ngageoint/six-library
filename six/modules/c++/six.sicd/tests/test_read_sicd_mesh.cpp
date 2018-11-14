#include <limits.h>
#include <iostream>
#include <vector>

#include <math/poly/Fit.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>

int main(int argc, char** argv)
{
    try
    {
        const std::string progname(argv[0]);
        if (argc != 2)
        {
            std::cerr << "Usage: " << sys::Path::basename(progname)
                      << " <SICD pathname>\n\n";
            return 1;
        }

        const std::string sicdPathname(argv[1]);
        std::vector<std::string> schemaPaths;
        const size_t orderX(4);
        const size_t orderY(4);
        std::auto_ptr<six::sicd::ComplexData> complexData;
        std::vector<std::complex<float> > widebandData;
        six::Poly2D outputRowColToSlantRow;
        six::Poly2D outputRowColToSlantCol;
        std::auto_ptr<six::sicd::NoiseMesh> noiseMesh;

        std::cout << "Read SICD: " << sicdPathname << std::endl;
        six::sicd::Utilities::readSicd(sicdPathname,
                                       schemaPaths,
                                       orderX,
                                       orderY,
                                       complexData,
                                       widebandData,
                                       outputRowColToSlantRow,
                                       outputRowColToSlantCol,
                                       noiseMesh);

        std::cout << "outputRowColToSlantRow: " << outputRowColToSlantRow << std::endl;
        std::cout << "outputRowColToSlantCol: " << outputRowColToSlantCol << std::endl;

        const types::RowCol<double> outputCenter(
            complexData->radarCollection->area->plane->referencePoint.rowCol.row,
            complexData->radarCollection->area->plane->referencePoint.rowCol.col);

        const types::RowCol<double> slantCenter(
            complexData->imageData->scpPixel.row,
            complexData->imageData->scpPixel.col);

        std::cout << "outputRowColToSlantRow(outputCenter): " <<
            outputRowColToSlantRow(outputCenter.row, outputCenter.col) << 
            " ?== slantCenter.row: " << slantCenter.row << std::endl;

        std::cout << "outputRowColToSlantCol(outputCenter): " <<
            outputRowColToSlantCol(outputCenter.row, outputCenter.col) << 
            " ?== slantCenter.col: " << slantCenter.col << std::endl;

        // Write out mesh sizes.
        std::cout << "noiseMesh: dims: " << noiseMesh->getMeshDims().row << 
            ":" << noiseMesh->getMeshDims().col <<
            ":" << noiseMesh->getMeshDims().area() << std::endl;
        std::cout << "noiseMesh: getMainBeamNoise().size(): " << 
            noiseMesh->getMainBeamNoise().size() <<
            " ?== noiseMesh area: " << noiseMesh->getMeshDims().area() << std::endl;
        std::cout << "noiseMesh: getAzimuthAmbiguityNoise().size(): " <<
            noiseMesh->getAzimuthAmbiguityNoise().size() <<
            " ?== noiseMesh area: " << noiseMesh->getMeshDims().area() << std::endl;
        std::cout << "noiseMesh: getCombinedNoise().size(): " <<
            noiseMesh->getCombinedNoise().size() <<
            " ?== noiseMesh area: " << noiseMesh->getMeshDims().area() << std::endl;
        std::cout << std::endl;

        // Get the noise floor polynomial.
        six::Poly2D sicdNoisePoly = 
            complexData->radiometric->noiseLevel.noisePoly;

        std::cout << "sicdNoisePoly: " << sicdNoisePoly << std::endl;

        // Default: SICD 1.1.0
        bool polyIsDb = true;
        bool polyIsRelative = false;

        if (complexData->radiometric->noiseLevel.noiseType.empty())
        {
            // SICD 0.4.0
            polyIsDb = false;
            polyIsRelative = true;
        }

        if (complexData->radiometric->noiseLevel.noiseType == "RELATIVE")
        {
            // SICD 1.1.0.
            polyIsRelative = true;
        }

        std::cout << "polyIsDb: " << polyIsDb << std::endl;
        std::cout << "polyIsRelative: " << polyIsRelative << std::endl;

        // Compute noise floor polynomial from mesh.
        const types::RowCol<size_t> dims  = noiseMesh->getMeshDims();
        const std::vector<double>& x = noiseMesh->getX();
        const std::vector<double>& y = noiseMesh->getY();
        const std::vector<double>& noise = noiseMesh->getCombinedNoise();

        // Min, max variables.
        double noisePowerMin =  std::numeric_limits<double>::max();
        double noisePowerMax = -std::numeric_limits<double>::max();
        double noiseDbMin =  std::numeric_limits<double>::max();
        double noiseDbMax = -std::numeric_limits<double>::max();
        double xMin =  std::numeric_limits<double>::max();
        double xMax = -std::numeric_limits<double>::max();
        double yMin =  std::numeric_limits<double>::max();
        double yMax = -std::numeric_limits<double>::max();

        const size_t GDIM = 128;

        math::linear::Matrix2D<double> xMat(GDIM, GDIM, 0.0);
        math::linear::Matrix2D<double> yMat(GDIM, GDIM, 0.0);
        math::linear::Matrix2D<double> noiseMat(GDIM, GDIM, 0.0);

        // Populate matrices with location and noise data.
        size_t rowIncr = dims.row / GDIM;
        size_t colIncr = dims.col / GDIM;
        for (size_t ii = 0; ii < GDIM; ++ii)
        {
            size_t idx = ii * rowIncr * dims.col;
            for (size_t jj = 0; jj < GDIM; ++jj, idx += colIncr)
            {
                xMat(ii, jj) = x[idx];
                yMat(ii, jj) = y[idx];
                noiseMat(ii, jj) = noise[idx];

                // Keep track of min, max values.
                xMin = std::min(xMin, xMat(ii, jj));
                xMax = std::max(xMax, xMat(ii, jj));
                yMin = std::min(yMin, yMat(ii, jj));
                yMax = std::max(yMax, yMat(ii, jj));
                noisePowerMin = std::min(noisePowerMin, noiseMat(ii, jj));
                noisePowerMax = std::max(noisePowerMax, noiseMat(ii, jj));

                if (polyIsDb)
                {
                    noiseMat(ii, jj) = noiseMat(ii, jj) > 0 ? 
                                         10.0 * std::log10(noiseMat(ii, jj)) : 
                                         -150.0;

                    noiseDbMin = std::min(noiseDbMin, noiseMat(ii, jj));
                    noiseDbMax = std::max(noiseDbMax, noiseMat(ii, jj));
                }
            }
        }

        std::cout << "xMin: " << xMin << std::endl;
        std::cout << "xMax: " << xMax << std::endl;

        std::cout << "yMin: " << yMin << std::endl;
        std::cout << "yMax: " << yMax << std::endl;

        std::cout << "noisePowerMin: " << noisePowerMin << std::endl;
        std::cout << "noisePowerMax: " << noisePowerMax << std::endl;
        if (polyIsDb)
        {
            std::cout << "noiseDbMin: " << noiseDbMin << std::endl;
            std::cout << "noiseDbMax: " << noiseDbMax << std::endl;
        
        }
        std::cout << std::endl;

        // Fit a polynomial to the mesh locations and noise.
        six::Poly2D meshNoisePoly = math::poly::fit(
            xMat, yMat, noiseMat, sicdNoisePoly.orderX(), sicdNoisePoly.orderY());

        if (polyIsRelative)
        {
            meshNoisePoly[0][0] = 0.0;
        }

        std::cout << "meshNoisePoly: " << meshNoisePoly << std::endl;

        // Evaluate polynomials at grid locations and compute
        // RMS differences.
        double sicdPolyMeshSumDb = 0.0;
        double meshPolyMeshSumDb = 0.0;
        double sicdPolyMeshPolySumDb = 0.0;
        double sicdPolyMeshSumPower = 0.0;
        double meshPolyMeshSumPower = 0.0;
        double sicdPolyMeshPolySumPower = 0.0;
        for (size_t ii = 0; ii < GDIM; ++ii)
        {
            for (size_t jj = 0; jj < GDIM; ++jj)
            {
                double sicdPolyNoisePower;
                double meshPolyNoisePower;
                double meshNoisePower;
                double sicdPolyNoiseDb;
                double meshPolyNoiseDb;
                double meshNoiseDb;
                if (polyIsDb)
                {
                    // Evaluate polynomials.
                    sicdPolyNoiseDb = sicdNoisePoly(xMat(ii, jj), yMat(ii, jj));
                    meshPolyNoiseDb = meshNoisePoly(xMat(ii, jj), yMat(ii, jj));
                    meshNoiseDb = noiseMat(ii, jj);

                    // Convert to power.
                    sicdPolyNoisePower = pow(10.0, sicdPolyNoiseDb/10.0);
                    meshPolyNoisePower = pow(10.0, meshPolyNoiseDb/10.0);
                    meshNoisePower = pow(10.0, meshNoiseDb/10.0);
                }
                else
                {
                    // Evaluate polynomials.
                    sicdPolyNoisePower = sicdNoisePoly(xMat(ii, jj), yMat(ii, jj));
                    meshPolyNoisePower = meshNoisePoly(xMat(ii, jj), yMat(ii, jj));
                    meshNoisePower = noiseMat(ii, jj);

                    // Convert to dB.
                    sicdPolyNoiseDb = 10.0 * log10(sicdPolyNoisePower);
                    meshPolyNoiseDb = 10.0 * log10(meshPolyNoisePower);
                    meshNoiseDb = 10.0 * log10(meshNoisePower);
                }

                // Compute difference(s) squared and sum.
                double diff = (sicdPolyNoisePower - meshPolyNoisePower);
                sicdPolyMeshSumPower += diff * diff;
                diff = (meshPolyNoisePower - meshNoisePower);
                meshPolyMeshSumPower += diff * diff;
                diff = (sicdPolyNoisePower - meshPolyNoisePower);
                sicdPolyMeshPolySumPower += diff * diff;

                diff = (sicdPolyNoiseDb - meshPolyNoiseDb);
                sicdPolyMeshSumDb += diff * diff;
                diff = (meshPolyNoiseDb - meshNoiseDb);
                meshPolyMeshSumDb += diff * diff;
                diff = (sicdPolyNoiseDb - meshPolyNoiseDb);
                sicdPolyMeshPolySumDb += diff * diff;

            }
        }

        // Compute RMS values.
        double sicdPolyMeshRMSPower = std::sqrt(sicdPolyMeshSumPower / (GDIM * GDIM));
        double meshPolyMeshRMSPower = std::sqrt(meshPolyMeshSumPower / (GDIM * GDIM));
        double sicdPolyMeshPolyRMSPower = std::sqrt(sicdPolyMeshPolySumPower / (GDIM * GDIM));

        double sicdPolyMeshRMSDb = std::sqrt(sicdPolyMeshSumDb / (GDIM * GDIM));
        double meshPolyMeshRMSDb = std::sqrt(meshPolyMeshSumDb / (GDIM * GDIM));
        double sicdPolyMeshPolyRMSDb = std::sqrt(sicdPolyMeshPolySumDb / (GDIM * GDIM));

        std::cout << "Power Difference RMS:" << std::endl;
        std::cout << "    sicdPolyMeshRMS    : " << sicdPolyMeshRMSPower <<
            "(power) " << 10.0*log10(sicdPolyMeshRMSPower) << " (dB)" << std::endl;
        std::cout << "    meshPolyMeshRMS    : " << meshPolyMeshRMSPower <<
            "(power) " << 10.0*log10(meshPolyMeshRMSPower) << " (dB)" << std::endl;
        std::cout << "    sicdPolyMeshPolyRMS: " << sicdPolyMeshPolyRMSPower <<
            "(power) " << 10.0*log10(sicdPolyMeshPolyRMSPower) << " (dB)" << std::endl;

        std::cout << "dB Difference RMS:" << std::endl;
        std::cout << "    sicdPolyMeshRMS    : " << sicdPolyMeshRMSDb <<
            "(dB) " << pow(10.0, sicdPolyMeshRMSDb/10.0) << " (power)" << std::endl;
        std::cout << "    meshPolyMeshRMS    : " << meshPolyMeshRMSDb <<
            "(dB) " << pow(10.0, meshPolyMeshRMSDb/10.0) << " (power)" << std::endl;
        std::cout << "    sicdPolyMeshPolyRMS: " << sicdPolyMeshPolyRMSDb <<
            "(dB) " << pow(10.0, sicdPolyMeshPolyRMSDb/10.0) << " (power)" << std::endl;
    }   
    catch (const except::Exception& e)
    {
        std::cerr << e.getMessage() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }

    return 0;
}
