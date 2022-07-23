from nextcord import Intents
from nextcord.ext.commands import Bot
import os
from itertools import repeat
import core, test

MODULES = [core, test]
PM_MODULES = []
cmds = lambda mods: [fn for m in mods for nm, fn in m.__dict__.items() if 
    callable(fn) and not (nm.startswith('__') and nm.endswith('__'))]
intents = Intents.default()
intents.message_content = True
client = Bot(command_prefix='!', intents=intents)
client.remove_command('help')


def build(fn, tgt=lambda x:x):
    @client.command(name=fn.__name__)
    async def _(ctx, msg=''):
        await tgt(ctx).send(fn(msg))


tuple(map(build, cmds(MODULES)))
tuple(map(build, cmds(PM_MODULES), repeat(lambda x:x.author)))
client.run(os.environ['OPENMJ_DISCORD_TOKEN'])
