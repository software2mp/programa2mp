cmake_minimum_required(VERSION 2.6)

FIND_PACKAGE(SuriLib REQUIRED)

WXWIDGETS_ADD_RESOURCES(Application CHtmlToolbar.xrc 
GenericContextPanel.xrc HtmlViewer.xrc PatToolbar.xrc
Resources.xrc
VectorTool.xrc AnotationTool.xrc PatManager.xrc PatInfo.xrc
PhotoProperties.xrc HotLinkProperties.xrc ContextTools.xrc)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib/resources)
ADD_LIBRARY(resources ${RESOURCES_SOURCES})

