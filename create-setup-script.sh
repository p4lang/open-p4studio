#! /bin/bash

echo "export SDE=\"${PWD}\""
echo "export SDE_INSTALL=\"\${SDE}/install\""
echo "export LD_LIBRARY_PATH=\"\${SDE_INSTALL}/lib\""
echo "export PATH=\"\${SDE_INSTALL}/bin:\${PATH}\""
