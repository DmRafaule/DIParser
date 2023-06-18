cmake_minimum_required(VERSION 3.5)
project(DIParser VERSION 0.1 DESCRIPTION "Parser module" LANGUAGES CXX)


file( GLOB SOURCES **.cpp )

SetFlags()
SetLibraries( freetype assimp DIRenderer )
CreateLibrary( ${PROJECT_NAME} )