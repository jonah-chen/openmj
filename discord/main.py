import nextcord
from nextcord.ext import commands
import os
import test

MODULES = [test]
CMDS = [fn for m in MODULES for nm, fn in m.__dict__.items() if callable(
    fn) and not (nm.startswith('__') and nm.endswith('__'))]

intents = nextcord.Intents.default()
intents.message_content = True
client = commands.Bot(command_prefix='!', intents=intents)


def build(fn):
    @client.command(name=fn.__name__)
    async def _(ctx, msg=''):
        await ctx.send(fn(msg))


tuple(map(build, CMDS))
client.run(os.environ['OPENMJ_DISCORD_TOKEN'])
