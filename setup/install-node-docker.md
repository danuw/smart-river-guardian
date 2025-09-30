https://nodejs.org/en/download/current

# Docker has specific installation instructions for each operating system.
# Please refer to the official documentation at https://docker.com/get-started/
# Pull the Node.js Docker image:
docker pull node:24-alpine
# Create a Node.js container and start a Shell session:
docker run -it --rm --entrypoint sh node:24-alpine
# Verify the Node.js version:
node -v # Should print "v24.8.0".
# Verify npm version:
npm -v # Should print "11.6.0".