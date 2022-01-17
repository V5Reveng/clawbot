#!/bin/sh
echo -n "Project name: "
read -r PROJECT_NAME
if test -z "$PROJECT_NAME"; then
	echo "Project name required"
	exit 1
fi
if test -e "$PROJECT_NAME"; then
	echo "Project with that name already exists"
	exit 1
fi

echo -n "Project version [0.1.0]: "
read -r PROJECT_VERSION

if test -z "$PROJECT_VERSION"; then
	PROJECT_VERSION="0.1.0"
fi

cp -r template "$PROJECT_NAME" || exit
cd "$PROJECT_NAME" || exit
sed -i "s/EXTREMELY_LONG_STRING_TO_AVOID_UNINTENTIONAL_REPLACEMENTS_PROJECT_NAME/$PROJECT_NAME/" Makefile project.pros
sed -i "s/EXTREMELY_LONG_STRING_TO_AVOID_UNINTENTIONAL_REPLACEMENTS_PROJECT_VERSION/$PROJECT_VERSION/" Makefile project.pros
