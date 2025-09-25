#include "../headers/webserver.hpp"





void freeDirective(DirectiveNode *node)
{
	delete node;
}

void freeContext(ContextNode *node)
{
	for(size_t i = 0; i <node->Childs.size(); i++)
	{
		if (node->Childs[i]->typeNode==isDirective)
			freeDirective(dynamic_cast<DirectiveNode *>(node->Childs[i]));
		else
			freeContext(dynamic_cast<ContextNode*>(node->Childs[i]));
	}
	delete node;
}

void freeTree(BaseNode *root)
{
	freeContext(dynamic_cast<ContextNode *>(root));
}



void free_D_Array(char **env)
{
	if (!env)
		return;
	int i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
}

